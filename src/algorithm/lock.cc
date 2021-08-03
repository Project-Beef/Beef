#include "lock.hh"
#include <deque>

namespace beef::algorithm::lock
{
	void apply(player* player_, world* world_, gameupdatepacket* packet, bool reapply)
	{
		std::vector<math::vec2i> total;

		int id = packet->m_item_id;
		int max = 0;

		if (id == SMALLLOCK)
			max = 10;
		else if (id == BIGLOCK)
			max = 48;
		else
			max = 200;

		math::vec2i start_pos = { static_cast<int>(packet->m_int_x), static_cast<int>(packet->m_int_y) };
		std::deque<math::vec2i> nodes{ start_pos };

		while (nodes.size() < max)
		{
			std::deque<math::vec2i> temp;

			for (const auto& node : nodes)
			{
				std::vector<math::vec2i> neighbours;

				if (node.m_y + 1 < world_->m_height)
					neighbours.push_back({ node.m_x, node.m_y + 1 });

				if (node.m_x + 1 < world_->m_width)
					neighbours.push_back({ node.m_x + 1, node.m_y });

				if (node.m_y - 1 >= 0)
					neighbours.push_back({ node.m_x, node.m_y - 1 });

				if (node.m_x - 1 >= 0)
					neighbours.push_back({ node.m_x - 1, node.m_y });

				if (max != 10 && !reapply)
				{
					if (node.m_x - 1 >= 0 && node.m_y - 1 >= 0)
						neighbours.push_back({ node.m_x - 1, node.m_y - 1 });

					if (node.m_x + 1 < world_->m_width && node.m_y - 1 >= 0)
						neighbours.push_back({ node.m_x + 1, node.m_y - 1 });

					if (node.m_x + 1 < world_->m_width && node.m_y + 1 < world_->m_height)
						neighbours.push_back({ node.m_x + 1, node.m_y + 1 });

					if (node.m_x - 1 >= 0 && node.m_y + 1 < world_->m_height)
						neighbours.push_back({ node.m_x - 1, node.m_y + 1 });
				}

				for (const auto& neighbour : neighbours)
				{
					tile* tile_ = world_->get_tile(neighbour);

					if (!tile_)
						continue;

					const item& item_ = tile_->get_base_item();

					if (std::find(total.begin(), total.end(), neighbour) != total.end())
						continue;
					else if (tile_->is_locked())
						continue;
					else if (item_.m_action_type == ITEMTYPE_LOCK ||
							 item_.m_action_type == ITEMTYPE_MAIN_DOOR ||
							 item_.m_action_type == ITEMTYPE_BEDROCK)
						continue;
					else if (reapply && tile_->get_fg_uint() == BLANK)
						continue;

					temp.emplace_back(neighbour);
					total.emplace_back(neighbour);

					if (total.size() > max)
						goto done;
				}
			}

			if (nodes.empty())
				goto done;

			nodes.pop_front();

			if (!temp.empty())
			{
				auto begin = temp.begin();

				for (auto p = temp.end() - 1; p != begin; --p)
				{
					nodes.emplace_back(*p);
				}
			}
		}
	done:;

		int size = total.size();
		int count = 0;

		if (size > max)
			size = max;

		gameupdatepacket* visual_packet = static_cast<gameupdatepacket*>(malloc(sizeof(gameupdatepacket) + size * 2));
		memset(visual_packet, 0, sizeof(gameupdatepacket) + size * 2);

		visual_packet->m_type = PACKET_SEND_LOCK;
		visual_packet->m_flags |= 8;
		visual_packet->m_int_x = packet->m_int_x;
		visual_packet->m_int_y = packet->m_int_y;
		visual_packet->m_item_id = packet->m_item_id;
		visual_packet->m_owner_id = player_->get_user_id();
		visual_packet->m_data_size = size * 2;
		visual_packet->m_tiles_length = size;

		uintmax_t offset = 0;

		for (const auto& pos : total)
		{
			if (count >= max)
				break;

			tile* tile_ = world_->get_tile(pos);

			if (tile_)
			{
				if (pos.m_x == packet->m_int_x && pos.m_y == packet->m_int_y)
					continue;

				tile_->set_new_lock(player_->get_user_id(), start_pos);

				short lp = pos.m_x + pos.m_y * world_->m_width;
				memcpy(&visual_packet->m_data + offset, &lp, 2);
				count += 1;
				offset += 2;
			}
		}

		world_->get_tile(start_pos)->set_new_lock(player_->get_user_id());

		gameupdatepacket* pck = variantlist{ "OnPlayPositioned", "audio/use_lock.wav" }.pack();
		pck->m_net_id = player_->get_net_id();

		world_->broadcast([&](int, player* ply)
		{
			ply->send_packet(NET_MESSAGE_GAME_PACKET, pck, sizeof(gameupdatepacket) + pck->m_data_size, ENET_PACKET_FLAG_RELIABLE);
			ply->send_packet(NET_MESSAGE_GAME_PACKET, visual_packet, sizeof(gameupdatepacket) + visual_packet->m_data_size, ENET_PACKET_FLAG_RELIABLE);
		});
		
		free(pck);
		free(visual_packet);
	}
}
